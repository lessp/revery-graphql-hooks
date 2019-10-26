open Revery_Core;
open Revery;
open Revery.UI;

module type QueryConfig = {
  let query: string;

  type t;
  let parse: [> | `Assoc(list((string, Yojson.Basic.t)))] => t;
};

module type Query = {
  type t;

  type status =
    | Idle
    | Loading
    | Error
    | Data(t);

  type action =
    | Idle
    | Loading
    | Error
    | Data(t);

  let use:
    (
      Revery.UI.React.Hooks.t(
        (
          Revery.UI.React.Hooks.Reducer.t(action),
          Revery.UI.React.Hooks.Effect.t(Revery.UI.React.Hooks.Effect.onMount)
        ) =>
        'a,
        'b,
      ),
      ~url: string
    ) =>
    (action, Revery.UI.React.Hooks.t('a, 'b));
};

module Make = (G: QueryConfig) : (Query with type t = G.t) => {
  type t = G.t;

  type status =
    | Idle
    | Loading
    | Error
    | Data(G.t);

  type action =
    | Idle
    | Loading
    | Error
    | Data(G.t);

  let reducer = (action: action, state) =>
    switch (action) {
    | Idle => Idle
    | Loading => Loading
    | Error => Error
    | Data(obj) => Data(obj)
    };

  let initialState = Idle;

  let use = (hooks, ~url) => {
    let (state, dispatch, hooks) =
      React.Hooks.reducer(~initialState, reducer, hooks);

    let hooks =
      React.Hooks.effect(
        OnMount,
        () => {
          let body =
            `Assoc([("query", `String(G.query))]) |> Yojson.Safe.to_string;

          dispatch(Loading);

          Fetch.(
            fetch(
              ~meth=`POST,
              ~body,
              ~headers=[("Content-Type", "application/json")],
              url,
            )
            |> Lwt.map(
                 fun
                 | Ok({Response.body, _}) => {
                     let response =
                       Response.Body.toString(body)
                       |> Yojson.Basic.from_string
                       |> Yojson.Basic.Util.member("data")
                       |> G.parse;

                     dispatch(Data(response));
                   }
                 | _ => dispatch(Error),
               )
            |> ignore
          );

          None;
        },
        hooks,
      );

    (state, hooks);
  };
};
