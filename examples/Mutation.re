open Revery.UI;
open Revery.UI.Components;

module AddGreeting = [%graphql
  {|
    mutation addGreeting($greeting: String!) {
      addGreeting(greeting: $greeting)
    }
  |}
];

let%component make = () => {
  let%hook (addGreeting, status) =
    Graphql.useMutation(AddGreeting.definition, ());

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
        addGreeting(
          ~variables=AddGreeting.makeVariables(~greeting="Cheers", ()),
          (),
        )
      }
      title="Click to add"
    />
    <Theme.Typography.H1 text />
  </Center>;
};
