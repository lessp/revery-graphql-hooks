module Typography = {
  open Revery;
  open Revery.UI;

  module Font = {
    let semiBold = Font.Family.fromFile("OpenSans-Semibold.ttf");
    let regular = Font.Family.fromFile("OpenSans-Regular.ttf");
  };

  module H1 = {
    let make = (~text, ()) => {
      <Text
        text
        fontFamily=Font.semiBold
        fontSize=48.
        style=Style.[color(Colors.white)]
      />;
    };
  };

  module Link = {
    let make = (~text, ~active=false, ()) => {
      <Text
        text
        fontFamily=Font.regular
        fontSize=24.
        style=Style.[color(active ? Colors.black : Colors.white)]
      />;
    };
  };
};
