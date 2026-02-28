<?php
class Database
{
    private $conn;

    /**
     * Connette al database.
     * @return bool true se connessione riuscita, false altrimenti.
     */
    public function connect($host, $username, $password, $dbname)
    {
        $this->conn = new mysqli($host, $username, $password, $dbname);

        if ($this->conn->connect_errno) {
            return false;
        }

        // Imposta charset (opzionale ma consigliato)
        $this->conn->set_charset("utf8mb4");
        return true;
    }

    /**
     * Esegue una query SQL con prepared statements.
     * Funziona come mysqli_stmt_bind_param: query($sql, $types, ...$params)
     * @return mysqli_result|bool Result set o false se fallisce.
     */
    public function query($sql, $types = "", ...$params)
    {
        if (!$this->conn) {
            return false;
        }

        $stmt = $this->conn->prepare($sql);
        if (!$stmt) {
            return false;
        }

        // Se ci sono parametri, li binda dinamicamente
        if ($types && !empty($params)) {
            $refs = [];
            foreach ($params as $key => $value) {
                $refs[$key] = &$params[$key];
            }
            array_unshift($refs, $types);
            array_unshift($refs, $stmt);

            if (!call_user_func_array("mysqli_stmt_bind_param", $refs)) {
                $stmt->close();
                return false;
            }
        }

        // Esegui la query
        if (!$stmt->execute()) {
            $stmt->close();
            return false;
        }

        // Ottieni il result set (se presente)
        $result = $stmt->get_result();
        if ($result === false) {
            $stmt->close();
            return true; // Per query tipo INSERT/UPDATE senza result set
        }

        return $result;
    }

    /**
     * Restituisce il numero di righe del result set.
     * @return int numero di righe o -1 se fallisce
     */
    public function get_rows_number($result)
    {
        if (!$result instanceof mysqli_result) {
            return -1;
        }
        return $result->num_rows;
    }

    /**
     * Recupera una riga dal result set (stesso comportamento di fetch_assoc()).
     * @return array|null
     */
    public function get_row($result)
    {
        if (!$result instanceof mysqli_result) {
            return null;
        }
        return $result->fetch_assoc();
    }
}
