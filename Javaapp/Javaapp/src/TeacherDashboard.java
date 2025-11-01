import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class TeacherDashboard extends JFrame implements ActionListener {

    private JButton viewStudentBtn, markAttendanceBtn, viewAttendanceBtn, logoutBtn;

    public TeacherDashboard() {
        setTitle("Teacher Dashboard");
        setSize(400, 350);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new GridLayout(4, 1, 10, 10));

        viewStudentBtn = new JButton("View Students");
        markAttendanceBtn = new JButton("Mark Attendance");
        viewAttendanceBtn = new JButton("View Attendance");
        logoutBtn = new JButton("Logout");

        viewStudentBtn.addActionListener(this);
        markAttendanceBtn.addActionListener(this);
        viewAttendanceBtn.addActionListener(this);
        logoutBtn.addActionListener(this);

        add(viewStudentBtn);
        add(markAttendanceBtn);
        add(viewAttendanceBtn);
        add(logoutBtn);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == viewStudentBtn) {
            new ViewStudentsForm().setVisible(true);
        } else if (e.getSource() == markAttendanceBtn) {
            new MarkAttendanceForm().setVisible(true);
        } else if (e.getSource() == viewAttendanceBtn) {
            new ViewAttendanceForm().setVisible(true);
        } else if (e.getSource() == logoutBtn) {
            this.dispose();
            new LoginForm().setVisible(true);
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new TeacherDashboard().setVisible(true));
    }
}
