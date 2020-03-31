open Revery;

let init = app => {
  let win =
    App.createWindow(
      app,
      ~createOptions=WindowCreateOptions.create(~width=1024, ~height=640, ()),
      "Revery GraphQL Hooks - Example",
    );

  let _startEventLoop = Revery_Lwt.startEventLoop();

  let _start = UI.start(win, <Main app />);

  ();
};

App.start(init);
