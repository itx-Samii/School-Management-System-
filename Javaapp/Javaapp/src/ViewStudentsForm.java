import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.io.*;
import java.util.Scanner;

public class    ViewStudentsForm extends JFrame {

    private JTable table;
    private DefaultTableModel model;

    public ViewStudentsForm() {
        setTitle("View Students");
        setSize(600, 400);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);

        model = new DefaultTableModel();
        model.setColumnIdentifiers(new String[]{"ID", "Name", "Class"});
        table = new JTable(model);
        add(new JScrollPane(table), BorderLayout.CENTER);

        loadStudents();
    }

    private void loadStudents() {
        try {
            File file = new File("E:/School Managment System/CppBackend/students.txt");
            if (!file.exists()) {
                JOptionPane.showMessageDialog(this, "students.txt not found at:\n" + file.getAbsolutePath());
                return;
            }

            Scanner reader = new Scanner(file);
            while (reader.hasNextLine()) {
                String line = reader.nextLine().trim();
                if (line.isEmpty()) continue;

                String[] data = line.split(",");
                // we expect at least 3 values: ID, Name, Class
                if (data.length >= 3) {
                    String id = data[0].trim();
                    String name = data[1].trim();
                    String className = data[2].trim();
                    model.addRow(new Object[]{id, name, className});
                }
            }
            reader.close();

            if (model.getRowCount() == 0) {
                JOptionPane.showMessageDialog(this, "No student records found in file.");
            }

        } catch (Exception e) {
            JOptionPane.showMessageDialog(this, "Error loading students: " + e.getMessage());
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new ViewStudentsForm().setVisible(true));
    }
}
