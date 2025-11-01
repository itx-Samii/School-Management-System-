import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.util.Scanner;

public class LoginForm extends JFrame implements ActionListener {
    private JComboBox<String> userType;
    private JTextField usernameField;
    private JPasswordField passwordField;
    private JButton loginButton, exitButton;

    public LoginForm() {
        setTitle("School Management System - Login");
        setSize(400, 300);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout());

        JLabel title = new JLabel("SCHOOL MANAGEMENT SYSTEM", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 16));
        add(title, BorderLayout.NORTH);

        JPanel panel = new JPanel(new GridLayout(5, 2, 10, 10));
        panel.setBorder(BorderFactory.createEmptyBorder(20, 40, 20, 40));

        panel.add(new JLabel("User Type:"));
        userType = new JComboBox<>(new String[]{"Admin", "Teacher", "Student", "Parent"});
        panel.add(userType);

        panel.add(new JLabel("Username:"));
        usernameField = new JTextField();
        panel.add(usernameField);

        panel.add(new JLabel("Password:"));
        passwordField = new JPasswordField();
        panel.add(passwordField);

        loginButton = new JButton("Login");
        loginButton.addActionListener(this);
        panel.add(loginButton);

        exitButton = new JButton("Exit");
        exitButton.addActionListener(this);
        panel.add(exitButton);

        add(panel, BorderLayout.CENTER);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == loginButton) {
            String user = (String) userType.getSelectedItem();
            String username = usernameField.getText().trim();
            String password = new String(passwordField.getPassword()).trim();

            if (username.isEmpty() || password.isEmpty()) {
                JOptionPane.showMessageDialog(this, "Please fill in all fields.");
                return;
            }

            if (user.equals("Admin")) {
                if (username.equals("admin") && password.equals("admin123")) {
                    new AdminDashboard().setVisible(true);
                    this.dispose();
                } else {
                    JOptionPane.showMessageDialog(this, "Invalid admin credentials!");
                }
                return;
            }

            if (user.equals("Teacher")) {
                if (username.equals("teacher") && password.equals("teacher123")) {
                    new TeacherDashboard().setVisible(true);
                    this.dispose();
                } else {
                    JOptionPane.showMessageDialog(this, "Invalid teacher credentials!");
                }
                return;
            }

            if (user.equals("Parent")) {
                // Simple parent login (you can change logic later)
                if (username.equals("parent") && password.equals("parent123")) {
                    // you can create a ParentDashboard that asks for child's ID
                    JOptionPane.showMessageDialog(this, "Welcome Parent! (Parent dashboard not implemented)");
                } else {
                    JOptionPane.showMessageDialog(this, "Invalid parent credentials!");
                }
                return;
            }

            if (user.equals("Student")) {
                boolean found = false;
                try {
                    File file = new File("E:/School Managment System/CppBackend/students.txt");
                    if (!file.exists()) {
                        JOptionPane.showMessageDialog(this, "students.txt not found at: " + file.getAbsolutePath());
                        return;
                    }
                    Scanner reader = new Scanner(file);
                    while (reader.hasNextLine()) {
                        String line = reader.nextLine().trim();
                        if (line.isEmpty()) continue;
                        String[] data = line.split(",");
                        // expected format: ID,Name,Class,Password
                        if (data.length >= 4) {
                            String fileID = data[0].trim();
                            String filePassword = data[3].trim();
                            if (username.equals(fileID) && password.equals(filePassword)) {
                                found = true;
                                new StudentDashboard(fileID).setVisible(true);
                                this.dispose();
                                break;
                            }
                        }
                    }
                    reader.close();
                    if (!found) {
                        JOptionPane.showMessageDialog(this, "Invalid Student ID or Password!");
                    }
                } catch (Exception ex) {
                    JOptionPane.showMessageDialog(this, "Error reading students.txt: " + ex.getMessage());
                }
            }
        } else if (e.getSource() == exitButton) {
            System.exit(0);
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new LoginForm().setVisible(true));
    }
}
