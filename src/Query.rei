open Revery.UI.React;

include (module type of S);

module type Query = {
  type t('responseType) = 'responseType;

  type status('responseType) =
    | Idle
    | Loading
    | Error
    | Data(t('responseType));

  /** Returns the status of the query

      {2 Example}
      {[
       let%hook status =
         Graphql.useQuery(
           ~variables=HelloName.makeVariables(~name="Kim", ()),
           HelloName.definition,
           (),
         );

       /* somewhere further down your component */
       let text =
         switch (status) {
         | Idle => "Idle"
         | Data(query) => query#helloName
         | Loading => "Loading..."
         | Error => "Error"
         };
      ]}
  */
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

module Make: (BC: BaseConfig) => Query;
