# Revery GraphQL Hooks

> A WIP library for hooks-based data-fetching with [GraphQL PPX](https://github.com/baransu/graphql_ppx_re) for [Revery](https://github.com/revery-ui/revery).

## Trying it out (in its current form)

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

module HelloQuery = ReveryGraphqlHooks.Query.Make(HelloQueryConfig);

let%component make = () => {
  let%hook status = HelloQuery.use(~url=Config.apiUrl);

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
    mutation addGreeting($greeting: String!) {
      addGreeting(greeting: $greeting)
    }
  |}
];

module AddGreetingMutation =
  ReveryGraphqlHooks.Mutation.Make(AddGreetingConfig);

let%component make = () => {
  let%hook (addGreetingMutation, status) =
    AddGreetingMutation.use(~url=Config.apiUrl);

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
