# Revery GraphQL Hooks

A library for easy handling of GraphQL within Revery using [graphql_ppx](https://github.com/reasonml-community/graphql_ppx).

## Table of Contents

1. [Getting started](#getting-started)
2. [Syntax](#syntax)
3. [Todo](#todo)
4. [Contributing](#contributing)
5. [License](#license)

## Getting started

### Installing

In your `package.json/esy.json` add:

```json
"dependencies": {
  ...
  "graphql_ppx": "reasonml-community/graphql_ppx:esy.json",
  "revery": "revery-ui/revery",
  "revery-graphql-hooks": "lessp/revery-graphql-hooks",
}
```

You will also need to copy anu `resolutions` in [example.json](example.json) except for `revery-graphql-hooks`.

then in your `dune`-file:

```lisp
(preprocess
  (pps
    graphql_ppx ;; + any other preprocessors (e.g. brisk-reconciler.ppx) for Revery
  ))
(libraries
  ;; any other libraries
  Revery Revery.lwt revery-graphql-hooks)
```

### Setup

Create a file, lets name it `Graphql.re` for easy access.

In this file we'll specify some settings for our HTTP-calls.

(If you'd like to try out the example below, you can use: `https://hello-graphql-api.lessp.now.sh/api`)

```ocaml
module Config = {
  let baseUrl = "https://your_graphql_api_endpoint.com/";
};

include ReveryGraphqlHooks.Make(Config);
```

**NOTE:** For Revery to handle Promises we need to start the event loop. Add the following line, prior to calling `UI.start`.

```re
let _startEventLoop = Revery_Lwt.startEventLoop();
```

That's all, we can now make some queries!

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

## ToDo

- [x] Propagate updates to hooks with the same queries
- [ ] Cache
- [ ] Simplify API by using `definition` from `graphql_ppx`

## Contributing

Contributions are more than welcome! Start by cloning this repository. The runnable example is located in [examples/](examples/), the library itself is located in [src/](src/).

```
# to build the library
esy

# to run the examples
esy '@example' start
```

## License

This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details
