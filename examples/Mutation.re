open Revery.UI;
open Revery.UI.Components;

module AddGreetingConfig = [%graphql
  {|
    mutation addGreeting($greeting: String!) {
      addGreeting(greeting: $greeting)
    }
  |}
];

let%component make = () => {
  let%hook (addGreetingMutation, status) =
    Graphql.useMutation(AddGreetingConfig.definition, ());

  let text =
    switch (status) {
    | Idle => "Idle"
    | Data(query) => query#addGreeting
    | Loading => "Loading..."
    | Error => "Error"
    };

  <Center>
    <Button
      fontFamily=Theme.Typography.Font.regular
      onClick={_ =>
        addGreetingMutation(
          ~variables=AddGreetingConfig.makeVariables(~greeting="Cheers", ()),
          (),
        )
      }
      title="Click to add"
    />
    <Theme.Typography.H1 text />
  </Center>;
};
