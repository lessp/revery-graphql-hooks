module Typography = {
  open Revery;
  open Revery.UI;

  let h1 =
    Style.[
      color(Colors.white),
      fontFamily("OpenSans-Semibold.ttf"),
      fontSize(48),
    ];

  let link =
    Style.[
      color(Colors.white),
      fontFamily("OpenSans-Regular.ttf"),
      fontSize(24),
    ];

  let linkActive =
    Style.[
      color(Colors.black),
      fontFamily("OpenSans-Regular.ttf"),
      fontSize(24),
    ];
};
