open Revery.UI.React;

include S;

module type Query = {
  type t('responseType) = 'responseType;

  type status('responseType) =
    | Idle
    | Loading
    | Error
    | Data(t('responseType));

  let useQuery:
    (
      ~variables: Yojson.Basic.t=?,
      (Yojson.Basic.t => t('responseType), string, 'b),
      unit,
      Hooks.t(
        (
          Hooks.Reducer.t(status('responseType)),
          Hooks.Effect.t(option(Yojson.Basic.t))
        ) =>
        'c,
        'd,
      )
    ) =>
    (status('responseType), Hooks.t('c, 'd));
};

module Make = (C: BaseConfig) : Query => {
  let baseUrl = C.baseUrl;
  let headers = C.headers;

  type t('responseType) = 'responseType;

  type status('responseType) =
    | Idle
    | Loading
    | Error
    | Data(t('responseType));

  type action('responseType) =
    | FetchData
    | Error
    | Data('responseType);

  let reducer = (action, _state): status('responseType) =>
    switch (action) {
    | FetchData => Loading
    | Error => Error
    | Data(obj) => Data(obj)
    };

  let initialState: status('responseType) = Idle;

  let createBody = (~variables=`Assoc([]), ~query) => {
    let payload =
      `Assoc([("query", `String(query)), ("variables", variables)])
      |> Yojson.Basic.to_string;

    (payload, variables);
  };

  let useQuery = (~variables: option(Yojson.Basic.t)=?, definition, ()) => {
    let (parse, graphqlQuery, composeVariables) = definition;

    let%hook (state, dispatch) = Hooks.reducer(~initialState, reducer);

    let subscribeToStore = query =>
      Store.subscribe(
        ~query,
        graphqlJson => {
          let data =
            graphqlJson
            |> Yojson.Basic.from_string
            |> Yojson.Basic.Util.member("data")
            |> parse;

          dispatch(Data(data));
        },
      );

    let executeRequest = (query, variables) => {
      dispatch(FetchData);

      Fetch.post(
        ~body=query,
        ~headers=[("Content-Type", "application/json"), ...headers],
        baseUrl,
      )
      |> Lwt.map(
           fun
           | Ok({Fetch.Response.body, _}) =>
             Store.publish(~query, Fetch.Body.toString(body))
           | _ => dispatch(Error),
         )
      |> ignore;
    };

    let%hook () =
      Hooks.effect(
        OnMountAndIf(
          (prevVariables, nextVariables) => prevVariables != nextVariables,
          variables,
        ),
        () => {
          let (payload, variables) =
            createBody(~query=graphqlQuery, ~variables?);

          let unsubscribe = subscribeToStore(payload);
          executeRequest(payload, variables);

          Some(unsubscribe);
        },
      );

    state;
  };
};
