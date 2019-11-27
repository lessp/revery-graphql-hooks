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
        (Hooks.Reducer.t(status), Hooks.Effect.t(Hooks.Effect.onMount)) => 'a,
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
