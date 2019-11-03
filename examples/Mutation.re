open Revery.UI;
open Revery.UI.Components;

module AddGreetingConfig = [%graphql
  {|
    mutation addGreeting($greeting: String!) {
      addGreeting(greeting: $greeting)
    }
  |}
];

module AddGreetingMutation = Graphql.Mutation.Make(AddGreetingConfig);

let%component make = () => {
  let%hook (addGreetingMutation, status) = AddGreetingMutation.use();

  let text =
    switch (status) {
    | Idle => "Idle"
    | Data(query) => query#addGreeting
    | Loading => "Loading..."
    | Error => "Error"
    };

  <Center>
    <Button
      fontFamily="OpenSans-Regular.ttf"
      onClick={_ =>
        addGreetingMutation(
          ~variables=AddGreetingConfig.make(~greeting="Cheers", ())#variables,
          (),
        )
      }
      title="Click to add"
    />
    <Text style=Theme.Typography.h1 text />
  </Center>;
};
