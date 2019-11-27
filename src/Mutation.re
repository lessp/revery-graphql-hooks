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
          Hooks.State.t(bool),
          Hooks.State.t(Yojson.Basic.t),
          Hooks.Reducer.t(status),
          Hooks.Effect.t(bool)
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
          ~headers=[("Content-Type", "application/json"), ...headers],
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
            /** only run if the previous state and the next one is different,
             *  AND shouldFetch is true */
            prevShouldFetch
            !== nextShouldFetch
            && nextShouldFetch == true,
          shouldFetch,
        ),
        () => {
          makeRequest(~variables);

          None;
        },
      );

    /* the mutation-function we provide is just a trigger for fetching
     * and puts the variables in the state
     */
    let mutation = (~variables, ()) => {
      setVariables(_prevVariables => variables);
      setShouldFetch(_prevShouldFetch => true);
    };

    (mutation, state);
  };
};
