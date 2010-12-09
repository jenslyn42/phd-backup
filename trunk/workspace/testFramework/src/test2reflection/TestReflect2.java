package test2reflection;

import java.lang.reflect.*;

@SuppressWarnings("rawtypes")
public class TestReflect2 {

  public static void main(String[] args) throws Exception {
    TestReflect2.invoke("say", new Class[] {String.class, String.class}, new Object[]
             {new String("Hello"), new String("World")});
    /*
      output :
         Hello World
    */
  }


public static void invoke(String aMethod, Class[] params, Object[] args)
     throws Exception {
//    Class c = Class.forName(aClass);
	  Class11 c = new Class11();
    Method m = c.getClass().getDeclaredMethod(aMethod, params);
    Object i = c.getClass().newInstance();
    Object r = m.invoke(i, args);
    System.out.println(((Class11) r).say("Bye ", "World"));
  }
}

class Class11 {
  public Class11 say( String s1, String s2) throws InterruptedException {
    System.out.println(s1 + " " + s2);
    return new Class11();
  }
}
