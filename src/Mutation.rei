open Revery.UI.React;

include (module type of S);

module type Mutation = {
  type t('responseType) = 'responseType;

  type status('responseType) =
    | Idle
    | Loading
    | Error
    | Data(t('responseType));

  /** Returns a tuple with the mutate-function in the first position and the status in the second position

      {2 Example}
      {[
        let%hook (addGreetingMutation, status) = Graphql.useMutation(AddGreetingConfig.definition, ());

        /* somewhere further down your component */
        addGreetingMutation(
          ~variables=AddGreetingConfig.makeVariables(~greeting="Hello", ()),
          (),
        )
      ]}
  */
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

module Make: (BC: BaseConfig) => Mutation;
