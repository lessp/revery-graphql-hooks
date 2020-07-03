open Revery.UI;
open Revery.UI.Components;

module HelloNameConfig = [%graphql
  {|
    query helloName($name: String!) {
      helloName(name: $name)
    }
  |}
];

let%component make = () => {
  let%hook status =
    Graphql.useQuery(
      ~variables=HelloNameConfig.makeVariables(~name="Abc", ()),
      HelloNameConfig.definition,
      (),
    );

  let text =
    switch (status) {
    | Idle => "Idle"
    | Data(query) => query#helloName
    | Loading => "Loading..."
    | Error => "Error"
    };

  <Center> <Theme.Typography.H1 text /> </Center>;
};
