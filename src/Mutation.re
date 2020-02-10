open Revery.UI.React;

include S;

module type Mutation = {
  type t;

  type status =
    | Idle
    | Loading
    | Error
    | Data(t);

  let use:
    (
      unit,
      Hooks.t(
        (
          Hooks.State.t(Yojson.Basic.t),
          Hooks.Reducer.t(status),
          Hooks.Effect.t(Yojson.Basic.t)
        ) =>
        'a,
        'b,
      )
    ) =>
    (((~variables: Yojson.Basic.t, unit) => unit, status), Hooks.t('a, 'b));
};

module Make =
       (C: BaseConfig, G: MutationConfig)
       : (Mutation with type t = G.t) => {
  type t = G.t;
  let baseUrl = C.baseUrl;
  let headers = C.headers;

  type status =
    | Idle
    | Loading
    | Error
    | Data(G.t);

  type action =
    | Fetch
    | Error
    | Data(G.t);

  let reducer = (action, _state): status =>
    switch (action) {
    | Fetch => Loading
    | Error => Error
    | Data(obj) => Data(obj)
    };

  let initialState: status = Idle;

  let use = () => {
    let%hook (variables, setVariables) = Hooks.state(`Assoc([]));
    let%hook (state, dispatch) = Hooks.reducer(~initialState, reducer);

    let%hook () =
      Hooks.effect(
        If(
          (prevVariables, nextVariables) => prevVariables !== nextVariables,
          variables,
        ),
        () => {
          let query =
            `Assoc([("query", `String(G.query)), ("variables", variables)])
            |> Yojson.Basic.to_string;

          let unsubscribe =
            Store.subscribe(
              ~query,
              graphqlJson => {
                let data =
                  graphqlJson
                  |> Yojson.Basic.from_string
                  |> Yojson.Basic.Util.member("data")
                  |> G.parse;

                dispatch(Data(data));
              },
            );

          Some(unsubscribe);
        },
      );

    let makeRequest = (~variables) => {
      let requestBody =
        `Assoc([("query", `String(G.query)), ("variables", variables)])
        |> Yojson.Basic.to_string;

      dispatch(Fetch);

      Fetch.(
        post(
          ~body=requestBody,
          ~headers=[("Content-Type", "application/json"), ...headers],
          baseUrl,
        )
        |> Lwt.map(
             fun
             | Ok({Response.body, _}) => {
                 let query =
                   `Assoc([
                     ("query", `String(G.query)),
                     ("variables", variables),
                   ])
                   |> Yojson.Basic.to_string;

                 Store.publish(~query, Response.Body.toString(body));
               }
             | _ => dispatch(Error),
           )
      )
      |> ignore;
    };

    let mutation = (~variables, ()) => {
      setVariables(_prevVariables => variables);

      makeRequest(~variables);
    };

    (mutation, state);
  };
};
