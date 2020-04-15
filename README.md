# Revery GraphQL Hooks
<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-3-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

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

```ocaml
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

## Contributors ✨

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://lessp.dev/"><img src="https://avatars3.githubusercontent.com/u/17602389?v=4" width="100px;" alt=""/><br /><sub><b>Tom Ekander</b></sub></a><br /><a href="#ideas-lessp" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/lessp/revery-graphql-hooks/commits?author=lessp" title="Code">💻</a> <a href="https://github.com/lessp/revery-graphql-hooks/commits?author=lessp" title="Documentation">📖</a></td>
    <td align="center"><a href="https://twitter.com/rita_krutikova"><img src="https://avatars2.githubusercontent.com/u/5932274?v=4" width="100px;" alt=""/><br /><sub><b>Margarita Krutikova</b></sub></a><br /><a href="#ideas-MargaretKrutikova" title="Ideas, Planning, & Feedback">🤔</a></td>
    <td align="center"><a href="https://github.com/despairblue"><img src="https://avatars1.githubusercontent.com/u/927609?v=4" width="100px;" alt=""/><br /><sub><b>Danny Martini</b></sub></a><br /><a href="#ideas-despairblue" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/lessp/revery-graphql-hooks/commits?author=despairblue" title="Code">💻</a></td>
  </tr>
</table>

<!-- markdownlint-enable -->
<!-- prettier-ignore-end -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
