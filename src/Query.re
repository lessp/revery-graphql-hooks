open Revery.UI.React;

include S;

module type Query = {
  type t;

  type status =
    | Idle
    | Loading
    | Error
    | Data(t);

  let use:
    (
      ~variables: Yojson.Basic.t=?,
      unit,
      Hooks.t(
        (
          Hooks.Reducer.t(status),
          Hooks.Effect.t(Hooks.Effect.onMount),
          Hooks.Effect.t(option(Yojson.Basic.t))
        ) =>
        'a,
        'b,
      )
    ) =>
    (status, Hooks.t('a, 'b));
};

module Make = (C: BaseConfig, G: QueryConfig) : (Query with type t = G.t) => {
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

  let use = (~variables: option(Yojson.Basic.t)=?, ()) => {
    let%hook (state, dispatch) = Hooks.reducer(~initialState, reducer);

    let query = () => {
      let variables =
        switch (variables) {
        | Some(variables) => variables
        | None => `Assoc([])
        };

      let query =
        `Assoc([("query", `String(G.query)), ("variables", variables)])
        |> Yojson.Basic.to_string;

      (query, variables);
    };

    let subscribeToStore = query =>
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

    let executeRequest = (query, variables) => {
      dispatch(Fetch);

      Fetch.(
        post(
          ~body=query,
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

                 Store.publish(~query, Body.toString(body));
               }
             | _ => dispatch(Error),
           )
      )
      |> ignore;
    };

    /* TODO: use OnMountAndIf when Revery has been updated to latest brisk */
    let%hook () =
      Hooks.effect(
        OnMount,
        () => {
          let (query, variables) = query();
          let unsubscribe = subscribeToStore(query);

          executeRequest(query, variables);

          Some(unsubscribe);
        },
      );

    let%hook () =
      Hooks.effect(
        If(
          (prevVariables, nextVariables) => prevVariables != nextVariables,
          variables,
        ),
        () => {
          let (query, variables) = query();
          let unsubscribe = subscribeToStore(query);
          executeRequest(query, variables);

          Some(unsubscribe);
        },
      );

    state;
  };
};
