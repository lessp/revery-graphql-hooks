open Revery.UI;
open Revery.UI.Components;

let allRoutes = [`Query, `QueryWithVariables, `Mutation];
let routeToString =
  fun
  | `Query => "Query"
  | `QueryWithVariables => "Query with variables"
  | `Mutation => "Mutation";

let%component make = (~app: Revery.App.t, ()) => {
  let%hook (route, setRoute) = Hooks.state(`Query);

  let currentRoute =
    switch (route) {
    | `Query => <Query />
    | `QueryWithVariables => <QueryWithVariables />
    | `Mutation => <Mutation />
    };

  /** This exits the example after 5 seconds for CI testing purposes */
  let handleClose = _ => {
    Revery.App.quit(~code=0, app);
  };

  <Center>
    <Ticker onTick=handleClose tickRate={Seconds(5.0)} />
    <View style=Style.[marginTop(32)]>
      <Row>
        {allRoutes
         |> List.map(r => {
              <View style=Style.[marginRight(16)]>
                <Clickable onClick={_ => setRoute(_ => r)}>
                  <Text
                    style={
                      route == r
                        ? Theme.Typography.linkActive : Theme.Typography.link
                    }
                    text={r |> routeToString}
                  />
                </Clickable>
              </View>
            })
         |> React.listToElement}
      </Row>
    </View>
    currentRoute
  </Center>;
};
