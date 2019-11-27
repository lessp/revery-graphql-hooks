include S;

module Make = (C: BaseConfig) => {
  module Query = {
    module Make = Query.Make(C);
  };

  module Mutation = {
    module Make = Mutation.Make(C);
  };
};
