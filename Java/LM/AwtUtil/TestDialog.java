import java.lang.*;
import java.awt.*;
import AwtUtil.*;

class TestDialog extends CommonDialog {
  public static void main(String []args) {
    TestDialog d = new TestDialog( "Test Dialog", new String[] { "Yes", "No", "Cancel" });
    int choice = d.doModal();
    if (choice == 0) {
      d.OnYes();
    } else if (choice == 1) {
      d.OnNo();
    } else {
      d.OnCancel();
    }
  }
  public TestDialog(String title, String[] buttons) {
    super(title, buttons);
  }
  public void OnYes() {
    System.out.println("Yes");
    dispose();
  }
  public void OnNo() {
    System.out.println("No");
    dispose();
  }
  public void OnCancel() {
    System.out.println("Cancel");
    dispose();
  }
}
