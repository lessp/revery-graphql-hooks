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
  let%hook status = HelloQuery.use(~url=Config.apiUrl, hooks);

  let text = switch (status) {
  | Idle => "Idle"
  | Data(data) => data#hello#name
  | Loading => "Loading..."
  | Error => "Error"
  };

  <Text text />;
};
```

## License

This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details
