package test2reflection;

import java.lang.reflect.*;
import java.io.*;

public class TestReflect {

@SuppressWarnings({ "rawtypes", "unused" })
   public static void main(String s[]) throws Exception{
      String aClass;
      String aMethod;
      // we assume that called methods have no argument
      Class params[] = {};
      Object paramsObj[] = {};

      while (true) {
         aMethod = lineInput("Method: ");
         Class1 thisClass = new Class1();
         // get an instance
         Object iClass = thisClass.getClass().newInstance();
         // get the method
         Method thisMethod = thisClass.getClass().getDeclaredMethod(aMethod, params);
         // call the method
         System.out.println
            (thisMethod.invoke(iClass, paramsObj).toString());
      }
      /* Output examples:
            Class : Class1
            Method: class1Method2
            ### Class 1, Method2 ###

            Class : java.util.Date
            Method: toString
            Sat Aug 11 13:18:39 EDT 2007

            Class : java.util.Date
            Method: getTime
            1186852732140      
       */
   }


   public static  String lineInput (String prompt) throws IOException {
     BufferedReader input =
        new BufferedReader(new InputStreamReader(System.in));
     System.out.print(prompt);
     return input.readLine();
   }

}

class Class1 {
   public String class1Method1() {
      return "*** Class 1, Method1 ***";
   }

   public String class1Method2() {
      return "### Class 1, Method2 ###";
   }
}

