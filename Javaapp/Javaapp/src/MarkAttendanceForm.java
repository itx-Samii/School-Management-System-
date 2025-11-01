import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

public class MarkAttendanceForm extends JFrame implements ActionListener {
    private JTextField idField, dateField;
    private JComboBox<String> statusBox;
    private JButton saveButton, clearButton, backButton;

    public MarkAttendanceForm() {
        setTitle("Mark Attendance");
        setSize(400, 300);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLayout(new BorderLayout());

        JLabel title = new JLabel("Mark Student Attendance", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 18));
        add(title, BorderLayout.NORTH);

        JPanel panel = new JPanel(new GridLayout(4, 2, 10, 10));
        panel.setBorder(BorderFactory.createEmptyBorder(20, 40, 20, 40));

        panel.add(new JLabel("Student ID:"));
        idField = new JTextField();
        panel.add(idField);

        panel.add(new JLabel("Date (DD-MM-YYYY):"));
        dateField = new JTextField();
        panel.add(dateField);

        panel.add(new JLabel("Status:"));
        statusBox = new JComboBox<>(new String[]{"Present", "Absent"});
        panel.add(statusBox);

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
            String date = dateField.getText().trim();
            String status = (String) statusBox.getSelectedItem();

            if (id.isEmpty() || date.isEmpty()) {
                JOptionPane.showMessageDialog(this, "All fields are required!");
                return;
            }

            try {
                FileWriter writer = new FileWriter("E:/School Managment System/CppBackend/attendance.txt", true);
                writer.write(id + "," + date + "," + status + "\n");
                writer.close();

                JOptionPane.showMessageDialog(this, "Attendance marked successfully!");
                idField.setText("");
                dateField.setText("");
                statusBox.setSelectedIndex(0);
            } catch (IOException ex) {
                JOptionPane.showMessageDialog(this, "Error saving attendance: " + ex.getMessage());
            }
        } else if (e.getSource() == clearButton) {
            idField.setText("");
            dateField.setText("");
            statusBox.setSelectedIndex(0);
        } else if (e.getSource() == backButton) {
            this.dispose();
            new AdminDashboard().setVisible(true);
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new MarkAttendanceForm().setVisible(true));
    }
}
