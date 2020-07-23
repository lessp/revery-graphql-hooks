open Revery.UI;
open Revery.UI.Components;

module HelloName = [%graphql
  {|
    query helloName($name: String!) {
      helloName(name: $name)
    }
  |}
];

let%component make = () => {
  let%hook status =
    Graphql.useQuery(
      ~variables=HelloName.makeVariables(~name="Abc", ()),
      HelloName.definition,
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
