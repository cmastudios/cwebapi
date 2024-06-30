import logo from './logo.svg';
import './App.css';
import { useState, useEffect } from 'react';

function App() {
  const [users, setUsers] = useState([]);
  useEffect(() => {
    fetch("http://localhost:4000/api/user/")
    .then((response) => response.json())
    .then((data) => {
        console.log(data);
        setUsers(data);
    })
    .catch((err) => {
        console.log(err.message);
    });
  }, []);
  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p>
          Edit <code>src/App.js</code> and save to reload.
        </p>
        <h2>Users</h2>
        <table>
          <thead>
            <tr><th>ID</th><th>Username</th></tr>
          </thead>
          <tbody>
            {users.map((user) => {
              return (
                <tr key={user.id}><td>{user.id}</td><td>{user.username}</td></tr>
              );
            })}
          </tbody>
        </table>
        <a
          className="App-link"
          href="https://reactjs.org"
          target="_blank"
          rel="noopener noreferrer"
        >
          Learn React
        </a>
      </header>
    </div>
  );
}

export default App;
