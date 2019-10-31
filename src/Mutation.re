open Revery_Core;
open Revery;
open Revery.UI;

module type MutationConfig = {
  let query: string;

  type t;
  let parse: [> | `Assoc(list((string, Yojson.Basic.t)))] => t;
};

module type Mutation = {
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
      unit,
      Revery.UI.React.Hooks.t(
        (
          Revery_UI.React.Hooks.State.t(bool),
          Revery_UI.React.Hooks.State.t(Yojson.Basic.t),
          Revery_UI.React.Hooks.Reducer.t(status),
          Revery_UI.React.Hooks.Effect.t(bool)
        ) =>
        'a,
        'b,
      )
    ) =>
    (
      ((~variables: Yojson.Basic.t, unit) => unit, status),
      Revery.UI.React.Hooks.t('a, 'b),
    );
};

module Make =
       (C: {let baseUrl: string;}, G: MutationConfig)
       : (Mutation with type t := G.t) => {
  type t = G.t;
  let baseUrl = C.baseUrl;

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

  let use = () => {
    let%hook (shouldFetch, setShouldFetch) = Hooks.state(false);
    let%hook (variables, setVariables) = Hooks.state(`Assoc([]));
    let%hook (state, dispatch) = Hooks.reducer(~initialState, reducer);

    let makeRequest = (~variables) => {
      let body =
        `Assoc([("query", `String(G.query)), ("variables", variables)])
        |> Yojson.Basic.to_string;

      dispatch(Fetch);

      Fetch.(
        fetch(
          ~meth=`POST,
          ~body,
          ~headers=[("Content-Type", "application/json")],
          baseUrl,
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
                 setShouldFetch(_prevShouldFetch => false);
               }
             | _ => {
                 dispatch(Error);
                 setShouldFetch(_prevShouldFetch => false);
               },
           )
      )
      |> ignore;
    };

    let%hook () =
      Hooks.effect(
        If(
          (prevShouldFetch, nextShouldFetch) =>
            prevShouldFetch !== nextShouldFetch && nextShouldFetch == true,
          shouldFetch,
        ),
        () => {
          makeRequest(~variables);

          None;
        },
      );

    let mutation = (~variables, ()) => {
      setVariables(_prevVariables => variables);
      setShouldFetch(_prevShouldFetch => true);
    };

    (mutation, state);
  };
};
