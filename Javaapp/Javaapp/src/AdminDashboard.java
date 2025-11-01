import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class AdminDashboard extends JFrame implements ActionListener {

    private JButton addStudentBtn, viewStudentBtn, markAttendanceBtn, viewAttendanceBtn, logoutBtn;

    public AdminDashboard() {
        setTitle("Admin Dashboard");
        setSize(400, 400);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new GridLayout(5, 1, 10, 10));

        addStudentBtn = new JButton("Add Student");
        viewStudentBtn = new JButton("View Students");
        markAttendanceBtn = new JButton("Mark Attendance");
        viewAttendanceBtn = new JButton("View Attendance");
        logoutBtn = new JButton("Logout");

        addStudentBtn.addActionListener(this);
        viewStudentBtn.addActionListener(this);
        markAttendanceBtn.addActionListener(this);
        viewAttendanceBtn.addActionListener(this);
        logoutBtn.addActionListener(this);

        add(addStudentBtn);
        add(viewStudentBtn);
        add(markAttendanceBtn);
        add(viewAttendanceBtn);
        add(logoutBtn);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == addStudentBtn) {
            new AddStudentForm().setVisible(true);
        } else if (e.getSource() == viewStudentBtn) {
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
        SwingUtilities.invokeLater(() -> new AdminDashboard().setVisible(true));
    }
}
