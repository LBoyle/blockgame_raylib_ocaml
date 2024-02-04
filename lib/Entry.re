open Raylib;

let width = 800;
let height = 450;

type column_t = {
  height: float,
  position: Raylib.Vector3.t,
  color: Raylib.Color.t,
};

let setup = () => {
  init_window(
    width,
    height,
    "raylib [core] example - 3d camera first person",
  );
  let camera =
    Camera.create(
      Vector3.create(4.0, 2.0, 4.0),
      Vector3.create(0.0, 1.8, 0.0),
      Vector3.create(0.0, 1.0, 0.0),
      80.0,
      CameraProjection.Perspective,
    );

  let columns =
    List.init(
      20,
      _ => {
        let height = Float.of_int(get_random_value(1, 12));
        {
          height,
          position:
            Vector3.create(
              Float.of_int(get_random_value(-15, 15)),
              height /. 2.0,
              Float.of_int(get_random_value(-15, 15)),
            ),
          color:
            Color.create(
              get_random_value(20, 255),
              get_random_value(10, 55),
              30,
              255,
            ),
        };
      },
    );

  set_target_fps(60);
  (camera, columns);
};

let draw_all = (camera, columns) => {
  begin_drawing();
  clear_background(Color.raywhite);
  begin_mode_3d(camera);
  draw_plane(
    Vector3.create(0.0, 0.0, 0.0),
    Vector2.create(32.0, 32.0),
    Color.lightgray,
  );
  draw_cube(Vector3.create(-16.0, 2.5, 0.0), 1.0, 5.0, 32.0, Color.blue);
  draw_cube(Vector3.create(16.0, 2.5, 0.0), 1.0, 5.0, 32.0, Color.lime);
  draw_cube(Vector3.create(0.0, 2.5, 16.0), 32.0, 5.0, 1.0, Color.gold);
  List.iter(
    ({position, height, color}) => {
      draw_cube(position, 2.0, height, 2.0, color);
      draw_cube_wires(position, 2.0, height, 2.0, Color.maroon);
    },
    columns,
  );
  end_mode_3d();
  end_drawing();
};

let rec loop = ((camera, columns)) => {
  if (window_should_close()) {
    close_window();
  } else {
    update_camera(addr(camera), CameraMode.First_person);
  };
  draw_all(camera, columns);
  loop((camera, columns));
};
