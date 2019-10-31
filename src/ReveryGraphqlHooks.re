module Make = (C: {let baseUrl: string;}) => {
  let baseUrl = C.baseUrl;

  module Query = {
    module Make = Query.Make(C);
  };

  module Mutation = {
    module Make = Mutation.Make(C);
  };
};
