model foo
  Real x(start = 0);
  Real y(start = 1);
  parameter Real w = 10;
equation
  der(x) = -w * y;
  der(y) = w * x;
end foo;

