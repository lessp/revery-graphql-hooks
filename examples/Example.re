open Revery;

let init = app => {
  let _ignore = Revery.Log.listen((_, msg) => print_endline("LOG: " ++ msg));

  let win =
    App.createWindow(
      app,
      ~createOptions=WindowCreateOptions.create(~width=1024, ~height=640, ()),
      "Revery GraphQL Hooks - Example",
    );

  let _ignore = Revery_Lwt.startEventLoop();

  let _ignore = UI.start(win, <Main app />);

  ();
};

App.start(init);
