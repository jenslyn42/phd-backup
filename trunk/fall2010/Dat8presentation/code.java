public class JSNIExample {

  String myInstanceField;
  static int myStaticField;

  void instanceFoo(String s) {
    // use s
  }

  static void staticFoo(String s) {
    // use s
  }

  public native void bar(JSNIExample x, String s) /*-{
    // Call instance method instanceFoo() on this
    this.@com.google.gwt.examples.JSNIExample::instanceFoo(Ljava/lang/String;)(s);

    // Call instance method instanceFoo() on x
    x.@com.google.gwt.examples.JSNIExample::instanceFoo(Ljava/lang/String;)(s);

    // Call static method staticFoo()
    @com.google.gwt.examples.JSNIExample::staticFoo(Ljava/lang/String;)(s);

    // Read instance field on this
    var val = this.@com.google.gwt.examples.JSNIExample::myInstanceField;

    // Write instance field on x
    x.@com.google.gwt.examples.JSNIExample::myInstanceField = val + " and stuff";

    // Read static field (no qualifier)
    @com.google.gwt.examples.JSNIExample::myStaticField = val + " and stuff";
  }-*/;

} 
