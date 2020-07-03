open Revery.UI;
open Revery.UI.Components;

module HelloConfig = [%graphql {|
    query hello {
      hello
    }
  |}];

let%component make = () => {
  let%hook status = Graphql.useQuery(HelloConfig.definition, ());

  let text =
    switch (status) {
    | Idle => "Idle"
    | Data(query) => query#hello
    | Loading => "Loading..."
    | Error => "Error"
    };

  <Center> <Theme.Typography.H1 text /> </Center>;
};
