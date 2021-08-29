import React from "react";
import "./App.css";
import {
  BottomNavigation,
  BottomNavigationAction,
  makeStyles,
} from "@material-ui/core";
import NightsStayTwoToneIcon from "@material-ui/icons/NightsStayTwoTone";
import LinkedCameraTwoToneIcon from "@material-ui/icons/LinkedCameraTwoTone";
import NightPage from "./pages/NightPage";
import { BrowserRouter as Router, Switch, Route, Link } from "react-router-dom";
import CameraPage from "./pages/CameraPage";

const useStyles = makeStyles({
  stickToBottom: {
    width: "100%",
    position: "fixed",
    bottom: 0,
  },
});

function App() {
  const [value, setValue] = React.useState(0);
  const classes = useStyles();

  return (
    <Router>
      <>
        <Switch>
          <Route path="/" exact>
            <NightPage />
          </Route>
          <Route path="/night">
            <NightPage />
          </Route>
          <Route path="/camera">
            <CameraPage />
          </Route>
        </Switch>

        <BottomNavigation
          className={classes.stickToBottom}
          value={value}
          onChange={(event, newValue) => {
            setValue(newValue);
          }}
          showLabels={false}
        >
          <BottomNavigationAction
            showLabel={false}
            component={Link}
            to="/night"
            icon={<NightsStayTwoToneIcon />}
          />
          <BottomNavigationAction
            showLabel={false}
            component={Link}
            to="/camera"
            icon={<LinkedCameraTwoToneIcon />}
          />
        </BottomNavigation>
      </>
    </Router>
  );
}

export default App;
