import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

public class AddStudentForm extends JFrame implements ActionListener {
    private JTextField idField, nameField, classField;
    private JPasswordField passwordField;
    private JButton saveButton, clearButton, backButton;

    public AddStudentForm() {
        setTitle("Add New Student");
        setSize(400, 320);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLayout(new BorderLayout());

        JPanel panel = new JPanel(new GridLayout(5, 2, 10, 10));
        panel.setBorder(BorderFactory.createEmptyBorder(20, 40, 20, 40));

        panel.add(new JLabel("Student ID:"));
        idField = new JTextField();
        panel.add(idField);

        panel.add(new JLabel("Name:"));
        nameField = new JTextField();
        panel.add(nameField);

        panel.add(new JLabel("Class Name:"));
        classField = new JTextField();
        panel.add(classField);

        panel.add(new JLabel("Password:"));
        passwordField = new JPasswordField();
        panel.add(passwordField);

        saveButton = new JButton("Save");
        clearButton = new JButton("Clear");
        backButton = new JButton("Back");

        saveButton.addActionListener(this);
        clearButton.addActionListener(this);
        backButton.addActionListener(this);

        panel.add(saveButton);
        panel.add(clearButton);

        add(panel, BorderLayout.CENTER);
        add(backButton, BorderLayout.SOUTH);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == saveButton) {
            String id = idField.getText().trim();
            String name = nameField.getText().trim();
            String className = classField.getText().trim();
            String pass = new String(passwordField.getPassword()).trim();

            if (id.isEmpty() || name.isEmpty() || className.isEmpty()) {
                JOptionPane.showMessageDialog(this, "ID, Name and Class are required!");
                return;
            }

            // If admin left password blank, auto-generate one (optional)
            if (pass.isEmpty()) {
                pass = id + "123"; // default simple password
            }

            try {
                FileWriter writer = new FileWriter("E:/School Managment System/CppBackend/students.txt", true);
                writer.write(id + "," + name + "," + className + "," + pass + "\n");
                writer.close();

                JOptionPane.showMessageDialog(this, "Student added successfully! Password: " + pass);
                idField.setText("");
                nameField.setText("");
                classField.setText("");
                passwordField.setText("");

            } catch (IOException ex) {
                JOptionPane.showMessageDialog(this, "Error saving student: " + ex.getMessage());
            }

        } else if (e.getSource() == clearButton) {
            idField.setText("");
            nameField.setText("");
            classField.setText("");
            passwordField.setText("");
        } else if (e.getSource() == backButton) {
            this.dispose();
            new AdminDashboard().setVisible(true);
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new AddStudentForm().setVisible(true));
    }
}
