import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.io.File;
import java.util.Scanner;

public class StudentDashboard extends JFrame {

    private JLabel nameLabel;
    private JLabel classLabel;
    private JTable table;
    private DefaultTableModel model;
    private JButton logoutButton;
    private String studentID;

    public StudentDashboard(String studentID) {
        this.studentID = studentID;

        setTitle("Student Dashboard");
        setSize(600, 400);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout());

        JPanel infoPanel = new JPanel(new GridLayout(2, 1));
        nameLabel = new JLabel("Name: ");
        classLabel = new JLabel("Class: ");
        infoPanel.add(nameLabel);
        infoPanel.add(classLabel);
        infoPanel.setBorder(BorderFactory.createEmptyBorder(10, 20, 10, 20));
        add(infoPanel, BorderLayout.NORTH);

        model = new DefaultTableModel();
        model.setColumnIdentifiers(new String[]{"Date", "Status"});
        table = new JTable(model);
        add(new JScrollPane(table), BorderLayout.CENTER);

        logoutButton = new JButton("Logout");
        logoutButton.addActionListener(e -> {
            this.dispose();
            new LoginForm().setVisible(true);
        });
        add(logoutButton, BorderLayout.SOUTH);

        loadStudentInfo();
        loadAttendance();
    }

    private void loadStudentInfo() {
        try {
            File file = new File("E:/School Managment System/CppBackend/students.txt");
            if (!file.exists()) {
                JOptionPane.showMessageDialog(this, "students.txt not found at: " + file.getAbsolutePath());
                return;
            }

            Scanner reader = new Scanner(file);
            boolean found = false;
            while (reader.hasNextLine()) {
                String line = reader.nextLine().trim();
                if (line.isEmpty()) continue;

                String[] data = line.split(",");
                // file format: ID,Name,Class,Password  (we accept >=3)
                if (data.length >= 3) {
                    String id = data[0].trim();
                    String name = data[1].trim();
                    String className = data[2].trim();

                    if (id.equals(studentID)) {
                        nameLabel.setText("Name: " + name);
                        classLabel.setText("Class: " + className);
                        found = true;
                        break;
                    }
                }
            }
            reader.close();

            if (!found) {
                nameLabel.setText("Name: (not found)");
                classLabel.setText("Class: (not found)");
            }

        } catch (Exception e) {
            JOptionPane.showMessageDialog(this, "Error loading student info: " + e.getMessage());
        }
    }

    private void loadAttendance() {
        model.setRowCount(0);
        try {
            File file = new File("E:/School Managment System/CppBackend/attendance.txt");
            if (!file.exists()) {
                // no attendance file yet — not an error
                return;
            }

            Scanner reader = new Scanner(file);
            while (reader.hasNextLine()) {
                String line = reader.nextLine().trim();
                if (line.isEmpty()) continue;

                String[] data = line.split(",");
                // expected format: ID,Date,Status
                if (data.length >= 3) {
                    String id = data[0].trim();
                    String date = data[1].trim();
                    String status = data[2].trim();
                    if (id.equals(studentID)) {
                        model.addRow(new Object[]{date, status});
                    }
                }
            }
            reader.close();

        } catch (Exception e) {
            JOptionPane.showMessageDialog(this, "Error loading attendance: " + e.getMessage());
        }
    }

    public static void main(String[] args) {
        // quick test — change ID to a real ID from your students.txt
        SwingUtilities.invokeLater(() -> new StudentDashboard("101").setVisible(true));
    }
}
