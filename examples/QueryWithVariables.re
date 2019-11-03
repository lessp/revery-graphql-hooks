open Revery.UI;
open Revery.UI.Components;

module HelloNameConfig = [%graphql
  {|
    query helloName($name: String!) {
      helloName(name: $name)
    }
  |}
];

module HelloNameQuery = Graphql.Query.Make(HelloNameConfig);

let%component make = () => {
  let%hook status =
    HelloNameQuery.use(
      ~variables=HelloNameConfig.make(~name="Abc", ())#variables,
      (),
    );

  let text =
    switch (status) {
    | Idle => "Idle"
    | Data(query) => query#helloName
    | Loading => "Loading..."
    | Error => "Error"
    };

  <Center> <Text style=Theme.Typography.h1 text /> </Center>;
};
