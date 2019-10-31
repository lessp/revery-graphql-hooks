# Revery GraphQL Hooks

> A WIP library for hooks-based data-fetching with [GraphQL PPX](https://github.com/baransu/graphql_ppx_re) for [Revery](https://github.com/revery-ui/revery).

## Getting started (in its current form)

### Installing

In your `package.json/esy.json` add:

```json
"dependencies": {
  ...
  "revery-graphql-hooks": "lessp/revery-graphql-hooks",
}
```

then in your `dune`-file:

```lisp
(libraries ... revery-graphql-hooks)
```

### Setup

Create a file, lets name it `Graphql.re` for easy access.

In this file we'll specify some settings for our calls.

```ocaml
module Config = {
  let baseUrl = "https://your_graphql_api_endpoint.com/";
};

include ReveryGraphqlHooks.Make(Config);
```

That's it, we can now make some calls!

## Syntax

### Query

```ocaml
module HelloQueryConfig = [%graphql
  {|
    query Hello {
      hello {
        name
      }
    }
  |}
];

module HelloQuery = Graphql.Query.Make(HelloQueryConfig);

let%component make = () => {
  let%hook status = HelloQuery.use();

  let text = switch (status) {
  | Idle => "Idle"
  | Data(query) => query#hello#name
  | Loading => "Loading..."
  | Error => "Error"
  };

  <Text style=Theme.Typography.h1 text />;
};
```

### Mutation

```ocaml
module AddGreetingConfig = [%graphql
  {|
    mutation AddGreeting($greeting: String!) {
      addGreeting(greeting: $greeting)
    }
  |}
];

module AddGreetingMutation =
  Graphql.Mutation.Make(AddGreetingConfig);

let%component make = () => {
  let%hook (addGreetingMutation, status) =
    AddGreetingMutation.use();

  let text =
    switch (status) {
    | Idle => "Idle"
    | Data(query) => query#addGreeting
    | Loading => "Loading..."
    | Error => "Error"
    };

  <Center>
    <Button
      onClick={_ =>
        addGreetingMutation(
          ~variables=AddGreetingConfig.make(~greeting="Cheers", ())#variables,
          (),
        )
      }>
      <Text style=Theme.Typography.h1 text="Click to add" />
    </Clickable>
    <Text style=Theme.Typography.h1 text />
  </Center>;
};
```

## License

This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details
