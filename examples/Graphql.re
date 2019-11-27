let secretToken = "abc";

module Config = {
  let baseUrl = "https://hello-graphql-api.lessp.now.sh/api";
  let headers = [("Authorization", "Bearer " ++ secretToken)];
};

include ReveryGraphqlHooks.Make(Config);
