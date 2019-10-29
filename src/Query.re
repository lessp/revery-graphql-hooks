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
    | Fetch
    | Error
    | Data(t);

  let use:
    (
      ~url: string,
      ~variables: Yojson.Basic.t=?,
      unit,
      Revery.UI.React.Hooks.t(
        (
          Revery.UI.React.Hooks.Reducer.t(status),
          Revery.UI.React.Hooks.Effect.t(Revery.UI.React.Hooks.Effect.onMount)
        ) =>
        'a,
        'b,
      )
    ) =>
    (status, Revery.UI.React.Hooks.t('a, 'b));
};

module Make = (G: QueryConfig) : (Query with type t := G.t) => {
  type t = G.t;

  type status =
    | Idle
    | Loading
    | Error
    | Data(G.t);

  type action =
    | Idle
    | Fetch
    | Error
    | Data(G.t);

  let reducer = (action, state): status =>
    switch (action) {
    | Idle => Idle
    | Fetch => Loading
    | Error => Error
    | Data(obj) => Data(obj)
    };

  let initialState: status = Idle;

  let use = (~url, ~variables: option(Yojson.Basic.t)=?, ()) => {
    let%hook (state, dispatch) = Hooks.reducer(~initialState, reducer);

    let%hook () =
      Hooks.effect(
        OnMount,
        () => {
          let variables =
            switch (variables) {
            | Some(variables) => variables
            | None => `Assoc([])
            };
          let body =
            `Assoc([("query", `String(G.query)), ("variables", variables)])
            |> Yojson.Basic.to_string;

          Console.log(("Body", body));

          dispatch(Fetch);

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
          )
          |> ignore;

          None;
        },
      );

    state;
  };
};
