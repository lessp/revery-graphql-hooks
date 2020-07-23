open Revery.UI.React;

include S;

module type Mutation = {
  type t('responseType) = 'responseType;

  type status('responseType) =
    | Idle
    | Loading
    | Error
    | Data(t('responseType));

  let useMutation:
    (
      ~variables: Yojson.Basic.t=?,
      (Yojson.Basic.t => t('responseType), string, 'b),
      unit,
      Hooks.t(
        (
          Hooks.State.t(Yojson.Basic.t),
          Hooks.Reducer.t(status('responseType)),
          Hooks.Effect.t(Yojson.Basic.t)
        ) =>
        'c,
        'd,
      )
    ) =>
    (
      ((~variables: Yojson.Basic.t, unit) => unit, status('responseType)),
      Hooks.t('c, 'd),
    );
};

module Make = (C: BaseConfig) : Mutation => {
  let baseUrl = C.baseUrl;
  let headers = C.headers;
  type t('responseType) = 'responseType;

  type status('responseType) =
    | Idle
    | Loading
    | Error
    | Data(t('responseType));

  type action('responseType) =
    | Fetch
    | Error
    | Data('responseType);

  let reducer = (action, _state): status('responseType) =>
    switch (action) {
    | Fetch => Loading
    | Error => Error
    | Data(obj) => Data(obj)
    };

  let initialState: status('responseType) = Idle;

  let useMutation = (~variables: option(Yojson.Basic.t)=?, definition, ()) => {
    let%hook (variables, setVariables) = Hooks.state(`Assoc([]));
    let%hook (state, dispatch) = Hooks.reducer(~initialState, reducer);

    let (parseQuery, graphqlQuery, composeVariables) = definition;

    let%hook () =
      Hooks.effect(
        If(
          (prevVariables, nextVariables) => prevVariables !== nextVariables,
          variables,
        ),
        () => {
          let query =
            `Assoc([
              ("query", `String(graphqlQuery)),
              ("variables", variables),
            ])
            |> Yojson.Basic.to_string;

          let unsubscribe =
            Store.subscribe(
              ~query,
              graphqlJson => {
                let data =
                  graphqlJson
                  |> Yojson.Basic.from_string
                  |> Yojson.Basic.Util.member("data")
                  |> parseQuery;

                dispatch(Data(data));
              },
            );

          Some(unsubscribe);
        },
      );

    let makeRequest = (~variables) => {
      let requestBody =
        `Assoc([
          ("query", `String(graphqlQuery)),
          ("variables", variables),
        ])
        |> Yojson.Basic.to_string;

      dispatch(Fetch);

      Fetch.post(
        ~body=requestBody,
        ~headers=[("Content-Type", "application/json"), ...headers],
        baseUrl,
      )
      |> Lwt.map(
           fun
           | Ok({Fetch.Response.body, _}) => {
               let query =
                 `Assoc([
                   ("query", `String(graphqlQuery)),
                   ("variables", variables),
                 ])
                 |> Yojson.Basic.to_string;

               Store.publish(~query, Fetch.Body.toString(body));
             }
           | _ => dispatch(Error),
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
