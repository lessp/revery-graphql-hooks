include S;

module Make = (C: BaseConfig) => {
  include Mutation.Make(C);
  include Query.Make(C);
};
