open Revery.UI;
open Revery.UI.Components;

module HelloConfig = [%graphql {|
    query hello {
      hello
    }
  |}];

module HelloQuery = Graphql.Query.Make(HelloConfig);

let%component make = () => {
  let%hook status = HelloQuery.use();

  let text =
    switch (status) {
    | Idle => "Idle"
    | Data(query) => query#hello
    | Loading => "Loading..."
    | Error => "Error"
    };

  <Center> <Text style=Theme.Typography.h1 text /> </Center>;
};
