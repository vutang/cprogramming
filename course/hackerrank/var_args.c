int min(int count,...) {
  int i;
  int min_value, a;

  // va_list is a type to hold information about
  // variable arguments
  va_list ap;

  // va_start must be called before accessing
  // variable argument list
  va_start(ap, arg_count);

  // Now arguments can be accessed one by one
  // using va_arg macro. Initialize min_value as first
  // argument in list
  min_value = va_arg(ap, int);

  // traverse rest of the arguments to find out minimum
  for (i = 2; i <= arg_count; i++)
    if ((a = va_arg(ap, int)) < min_value)
      min_value = a;

  // va_end should be executed before the function
  // returns whenever va_start has been previously
  // used in that function
  va_end(ap);

  return min_value;
}