open Revery.UI.React;

include (module type of S);

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

module Make: (BC: BaseConfig, QC: QueryConfig) => Query with type t := QC.t;
