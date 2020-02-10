module type BaseConfig = {
  let baseUrl: string;
  let headers: list((string, string));
};

module type MutationConfig = {
  let query: string;

  type t;
  let parse: Yojson.Basic.t => t;
};

module type QueryConfig = {
  let query: string;

  type t;
  let parse: Yojson.Basic.t => t;
};
