open Revery.UI.React;

include (module type of S);

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

module Make:
  (BC: BaseConfig, MC: MutationConfig) => Mutation with type t := MC.t;
