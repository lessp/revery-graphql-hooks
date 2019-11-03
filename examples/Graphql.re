module Config = {
  let baseUrl = "https://hello-graphql-api.lessp.now.sh/api";
};

include ReveryGraphqlHooks.Make(Config);
