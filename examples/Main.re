open Revery;
open Revery.UI;
open Revery.UI.Components;

let allRoutes = [`Query, `QueryWithVariables, `Mutation, `MutationTwo];
let routeToString =
  fun
  | `Query => "Query"
  | `QueryWithVariables => "Query with variables"
  | `Mutation => "Mutation"
  | `MutationTwo => "MutationTwo";

let%component make = (~app: Revery.App.t, ()) => {
  let%hook (route, setRoute) = Hooks.state(`Query);

  let currentRoute =
    switch (route) {
    | `Query => <Query />
    | `QueryWithVariables => <QueryWithVariables />
    | `Mutation => <Mutation />
    | `MutationTwo => <Mutation />
    };

  /** This exits the example after 5 seconds for CI testing purposes */
  let handleClose = _ => {
    Revery.App.quit(~code=0, app);
  };

  <Center>
    <Ticker onTick=handleClose tickRate={Time.seconds(5)} />
    <View style=Style.[marginTop(32)]>
      <Row>
        {allRoutes
         |> List.map(r => {
              <View style=Style.[marginRight(16)]>
                <Clickable onClick={_ => setRoute(_ => r)}>
                  <Theme.Typography.Link
                    active={route == r}
                    text={r |> routeToString}
                  />
                </Clickable>
              </View>
            })
         |> React.listToElement}
      </Row>
      currentRoute
    </View>
  </Center>;
};
